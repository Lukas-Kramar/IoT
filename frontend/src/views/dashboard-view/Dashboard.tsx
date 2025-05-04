import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { Alert, Badge, Button, Col, Container, Row, Spinner, Table } from "react-bootstrap";
import dayjs from "dayjs";
import organisationRequests, { Organisation } from "../../../API/requests/organisationRequests";
import OrganisationCard from "./components/organisationCard/OrganisationCard";
import { useOrganisationContext } from "../../customHooks/useOrganisationsContext";
import DeleteOrganisationModal from "./modals/DeleteOrganisationModal";
import UpdateOrganisationModal from "./modals/UpdateOrganisationModal";

export type DashboardModalVersion = 'update-organisation' | 'delete-organisation' | '';

const Dashboard = () => {
    const navigate = useNavigate();

    const { selectedOrganisation } = useOrganisationContext();

    const [isLoading, setIsLoading] = useState(false);
    const [modalVersion, setModalVersion] = useState<DashboardModalVersion>('');
    const [measurementPoints, setMeasurementPoints] = useState<[]>([]);




    if (!selectedOrganisation) {
        return (
            <Container className="mt-4">
                <h1>Dashboard</h1>
                <Alert variant="warning"> <b>Please select an organisation</b> to view the dashboard.</Alert>
            </Container>
        );
    }

    return (
        <>
            {modalVersion === 'update-organisation' && (
                <UpdateOrganisationModal
                    modalVersion={modalVersion}
                    setModalVersion={setModalVersion}
                    editedOrganisation={selectedOrganisation}
                />
            )}
            {modalVersion === 'delete-organisation' && (
                <DeleteOrganisationModal
                    modalVersion={modalVersion}
                    setModalVersion={setModalVersion}
                    editedOrganisation={selectedOrganisation}
                />
            )}

            <Container className="mt-4">
                <Row>
                    <Col sm={10}>
                        <h1>{selectedOrganisation.name}</h1>
                        <p className="text-muted">Organisation ID: {selectedOrganisation._id}</p>
                        <p>{selectedOrganisation.description}</p>
                    </Col>
                    <Col sm={2} className="d-flex justify-content-end gap-2 align-items-center">
                        <Button
                            variant="warning"
                            onClick={() => setModalVersion('update-organisation')}
                        >
                            <i className="bi bi-pencil-fill" />
                            <span className="ms-1">Edit</span>
                        </Button>
                        <Button
                            variant="danger"
                            onClick={() => setModalVersion('delete-organisation')}
                        >
                            <i className="bi bi-trash" />
                            <span className="ms-1">Delete</span>
                        </Button>
                    </Col>

                </Row>


            </Container>
        </>
    );
}

export default Dashboard;